const path = require("path");

const resolve = (...rest) => path.resolve(__dirname, "..", ...rest);

const root = resolve(".");
const src = resolve("src");

module.exports = {
	root,
	src,
	dist: resolve("dist"),
	editorEntry: resolve(src, "editor", "index.ts"),
	engineEntry: resolve(src, "index.ts"),
	resolve,
}