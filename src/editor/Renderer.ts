import { ECS } from "../ECS";
import { RendererComponent } from "../RendererComponent";
import { RenderSystem } from "../RenderSystem";
import { SubSystem } from "../SubSystem";

export class Renderer extends SubSystem<RendererProps>
{
	private _canvas: HTMLCanvasElement | null = null;

	public get canvas(): HTMLCanvasElement
	{
		if (!this._canvas)
			throw new Error(`Could not get canvas`);
		return this._canvas;
	}

	public init(props: RendererProps)
	{
		const ecs = this.engine.getSubSystem(ECS);
		// ecs.registerComponent(RendererComponent);
		ecs.registerSystem(RenderSystem);
		

		if (props.canvas)
			this.updateCanvas(props.canvas);
	}

	public readonly reset = () =>
	{
		this._canvas = null;
	}

	public readonly updateCanvas = (canvas: HTMLCanvasElement) =>
	{
		this._canvas = canvas;
	}
}

type RendererProps = {
	canvas?: HTMLCanvasElement
};