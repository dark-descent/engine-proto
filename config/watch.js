const { webpack } = require("webpack");
const config = require("./webpack.config");

webpack([config(true, true), config(false, true)]).watch({  }, (err, stats) => 
{
	if (err)
	{
		console.error(err);
	}
	else
	{
		console.log(stats.toString("minimal"));	
	}
});