import { Vector2 } from "../Vector2";
import { CoreComponent } from "../CoreComponent";

export default class Transform extends CoreComponent
{
	public get position(): Readonly<Vector2> 
	{
		return new Vector2();
	};

	public get rotation(): Readonly<Vector2>
	{
		return new Vector2();
	};

	public get scale(): Readonly<Vector2>
	{
		return new Vector2()
	};

	public set position(value: Vector2)
	{

	};

	public set rotation(value: Vector2)
	{

	};

	public set scale(value: Vector2)
	{

	};

	public get children(): Transform[]
	{
		return [];
	}
}