import { ComponentType } from "./Component";
import { RendererComponent, TestComponent } from "./RendererComponent";
import { System } from "./System";

export class RenderSystem extends System<RendererComponent>
{
	public get types(): ComponentType<RendererComponent>[]
	{
		return [
			RendererComponent,
			TestComponent
		];
	}

	public update(): void
	{
		
	}
}