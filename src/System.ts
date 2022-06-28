import { Component, ComponentType } from "./Component";

export abstract class System<T extends Component<any>>
{
	public abstract get types(): ComponentType<T>[];

	public abstract update(): void;
}

export type SystemType<T extends System<any>> = new () => T;