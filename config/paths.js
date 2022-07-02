const path = require("path");

const resolve = (...rest) => path.resolve(__dirname, "..", ...rest);

const root = resolve(".");
const src = resolve("src");

module.exports = {
	root,
	src,
	addonSrc: resolve(src, "addon"),
	resolve,
}