const { watch, readdirSync, writeFileSync } = require("fs");
const { resolve, addonSrc } = require("./paths");
const p = require("path");
const { spawn, spawnSync } = require("child_process");
const os = require("os");

const { existsSync } = require("fs");

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
				"cflags_cc": ["-std=c++20", "-fexceptions"],
				"win_delay_load_hook": os.platform() === "win32" ? "true" : "false",
				"sources": files.filter(f => f.endsWith("cpp")),
				"include_dirs": [
					resolve("src", "addon", "include"),
				],
				"conditions": [
					['OS=="mac"', {
						"xcode_settings": {
							'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
							"CLANG_CXX_LIBRARY": "libc++",
							"CLANG_CXX_LANGUAGE_STANDARD": "c++20",
							'MACOSX_DEPLOYMENT_TARGET': '10.14'
						}
					}],
					['OS=="win"', {
						"msvs_settings": {
							"VCCLCompilerTool": {
								"AdditionalOptions": ["-std:c++20", "/EHsc"],
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

console.log(`Configuring...`);
spawnSync(nodeGyp, ["configure", ...nodeGypArgs], { stdio: "inherit" });

runNodeGyp();

watch(addonSrc, { recursive: true }, runNodeGyp);
