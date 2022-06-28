import { Renderer } from "./Renderer";
import { Renderer as EditorRenderer } from "./editor/Renderer"
import { SubSystem } from "./SubSystem";
import { Component, ComponentData, ComponentType } from "./Component";
import { RendererComponent, TestComponent } from "./RendererComponent";
import { System, SystemType } from "./System";

@SubSystem.dependsOn([Renderer, EditorRenderer])
export class ECS extends SubSystem<any>
{
	private readonly entities: number[] = [];
	private readonly components: Map<ComponentType, Component<any>> = new Map();

	private readonly systems: Map<SystemType<any>, System<any>> = new Map();

	public init(props: any): void | Promise<void>
	{
		let entity = this.addEntity();
		this.setEntityComponent(entity,
			[RendererComponent, {}],
			[TestComponent, {}]
		);
		entity = this.addEntity();
		this.setEntityComponent(entity,
			[RendererComponent, {}],
			[TestComponent, {}]
		);
		entity = this.addEntity();
		this.setEntityComponent(entity,
			[TestComponent, {}]
		);
		entity = this.addEntity();
		this.setEntityComponent(entity,
			[RendererComponent, {}],
		);
		entity = this.addEntity();
		this.setEntityComponent(entity,
			[RendererComponent, {}],
			[TestComponent, {}]
		);
	}

	public readonly registerComponent = (componentType: new (...args: any) => Component<any>) =>
	{
		if (!this.components.has(componentType))
			this.components.set(componentType, new componentType(this));
	}

	public readonly registerSystem = <T extends System<any>>(system: SystemType<T>) =>
	{
		if (!this.systems.has(system))
		{
			const s = new system();
			s.types.forEach(type => this.registerComponent(type));
			this.systems.set(system, s);
		}
	}

	public addEntity()
	{
		return this.entities.push(0) - 1;
	}

	public setEntityComponent(entity: number, ...components: ComponentData<any>[])
	{
		components.forEach(([type, data]) => 
		{
			const c = this.components.get(type);
			if (!c)
				throw new Error(``);
			c.addComponent(entity, data);
			this.entities[entity] |= c.id;
		});
	}

	public updateSystem = <T extends System<any>>(system: SystemType<T>) => 
	{
		const s = this.systems.get(system);
		if (s)
		{
			const ids = s.types.map(type => this.components.get(type)?.id || 0).filter(s => s!==0);
			const entities: number[] = [];
			this.entities.forEach((e, i) => 
			{
				for(const id of ids)
					if((e & id) == 0)
						return;
				entities.push(i);
			});
			console.log(entities);
			s?.update();
		}
	};
}