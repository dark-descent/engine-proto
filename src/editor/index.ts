import { Engine } from "../Engine";

export namespace Editor
{
	const engine = new Engine();

	export const logEngine = () => console.log(engine);

	export const test = () => engine.test();
	
}