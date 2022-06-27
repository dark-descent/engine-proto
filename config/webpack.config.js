const TsconfigPathsPlugin = require("tsconfig-paths-webpack-plugin");
const webpack = require("webpack");
const { engineEntry, editorEntry, dist } = require("./paths");
const TypescriptDeclarationPlugin = require("typescript-declaration-webpack-plugin");

module.exports = (isEditorBuild = false, dev = false) => ({
	mode: dev ? "development" : "production",
	devtool: dev ? "inline-source-map" : undefined,
	entry: {
		engine: engineEntry,
		editor: {
			dependOn: "engine",
			import: editorEntry,
		},
	},
	output: {
		path: dist,
		filename: "[name].js",
		library: {
			name: "engine",
			type: "umd",
		  },
	},
	resolve: {
		extensions: [".tsx", ".ts", ".jsx", ".js", ".json"],
		plugins: [
			new TsconfigPathsPlugin({})
		]
	},
	module: {
		rules: [
			{
				test: /\.tsx?$/,
				loader: "ts-loader",
				exclude: /node_modules/
			}
		]
	},
	plugins: [
		new TypescriptDeclarationPlugin({
			out: `${isEditorBuild ? "editor" : "engine"}.d.ts`
		}),
		new webpack.DefinePlugin({
			env: JSON.stringify({
				isDev: dev,
				isEditorBuild
			})
		})
	]
});