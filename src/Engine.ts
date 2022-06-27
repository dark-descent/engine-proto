import { Guards } from "./Guards";

export class Engine
{
	public test() { console.log("Engine.test() called!"); }

	@Guards.editor
	public static readonly createEditorInstance = () =>
	{
		return new Engine();
	} 

	@Guards.game
	public static createGameInstance()
	{
		return new Engine();
	}
}