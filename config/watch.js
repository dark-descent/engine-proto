const { watch, readdirSync, writeFileSync, readFile, readFileSync, existsSync, statSync } = require("fs");
const { resolve, addonSrc } = require("./paths");
const p = require("path");
const { spawn } = require("child_process");
const os = require("os");
const crypto = require("crypto");

const isNum = (val) => val.length > 0 && !Number.isNaN(Number(val));

const getTarget = () =>
{
	const arg = process.argv[2];
	if (typeof arg === "string")
	{
		const splitted = arg.split(".");

		if (splitted.length === 3 && isNum(splitted[0]) && isNum(splitted[1]) && isNum(splitted[2]))
			return arg;

		const pkgPath = p.resolve(process.cwd(), arg, "package.json");

		if (existsSync(pkgPath))
			return require(pkgPath).devDependencies.electron.replace("^", "");
	}
	return null;
}

const target = getTarget();
const targetArg = target ? [`--target=${target}`] : [];
const nodeGypArgs = [
	...targetArg,
	"--arch=x64",
	"--dist-url=https://electronjs.org/headers",
	"-j", "max",
	"--debug"
];

const nodeGyp = `node-gyp${os.platform() === "win32" ? ".cmd" : ""}`;

const readRecursive = (path) =>
{
	const files = [];

	readdirSync(path, { withFileTypes: true }).forEach((val) =>
	{
		const dirEntry = p.join(path, val.name);
		if (val.isDirectory())
		{
			files.push(...readRecursive(dirEntry));
		}
		else
		{
			files.push(dirEntry);
		}
	});

	return files;
}

let nodeGypProc = null;

const runNodeGyp = () =>
{
	if (nodeGypProc)
		nodeGypProc.kill();

	const files = readRecursive(addonSrc);
	const json = JSON.stringify({
		"targets": [
			{
				"target_name": "addon",
				"msvs_version": "2019",
				"cflags_cc": ["-std=c++latest", "-fexceptions"],
				"win_delay_load_hook": os.platform() === "win32" ? "true" : "false",
				"sources": files.filter(f => f.endsWith("cpp")),
				"include_dirs": [
					resolve("src", "addon", "include"),
				],
				"conditions": [
					['OS=="mac"', {
						"xcode_settings": {
							"GCC_ENABLE_CPP_EXCEPTIONS": "YES",
							"CLANG_CXX_LIBRARY": "libc++",
							"CLANG_CXX_LANGUAGE_STANDARD": "c++latest",
							"MACOSX_DEPLOYMENT_TARGET": "10.14"
						}
					}],
					['OS=="win"', {
						"msvs_settings": {
							"VCCLCompilerTool": {
								"AdditionalOptions": ["-std:c++latest", "/EHsc"],
								"Optimization": 2
							},
						},
					}]
				]
			}
		]
	}, null, 4);

	writeFileSync(resolve("binding.gyp"), json, "utf-8");

	nodeGypProc = spawn(nodeGyp, ["rebuild", ...nodeGypArgs], { stdio: "inherit" });
}

runNodeGyp();


const getFileHashAsync = (path) => new Promise((res) => 
{
	readFile(path, "utf-8", (err, data) => 
	{
		res(crypto.createHash("md5").update(data).digest("base64"));
	});
});

const getFileHash = (path) => crypto.createHash("md5").update(readFileSync(path, "utf-8")).digest("base64");

const init = async () =>
{
	const srcPath = resolve("src", "addon", "src");
	const includePath = resolve("src", "addon", "include");
	const files = readRecursive(addonSrc).filter(file => file.startsWith(srcPath) || file.startsWith(includePath));
	filePromises = files.map(file => getFileHashAsync(file));
	const hashes = await Promise.all(filePromises);

	const fileHashes = {};
	hashes.forEach((hash, i) => fileHashes[files[i]] = hash);

	let onChangeTimeout = null;

	watch(addonSrc, { recursive: true }, (e, file) => 
	{
		try
		{
			const path = p.resolve(addonSrc, file);

			if (statSync(path).isDirectory())
				return;

			if (onChangeTimeout)
				clearTimeout(onChangeTimeout);
			onChangeTimeout = setTimeout(() => 
			{

				const hash = getFileHash(path);
				const oldHash = fileHashes[path];
				if (!oldHash || (oldHash !== hash))
				{
					runNodeGyp();
					fileHashes[path] = hash;
				}
			}, 150);
		}
		catch (e)
		{
			console.warn(e);
		}
	});
}

init();