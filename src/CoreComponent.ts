import { Component, ComponentType } from "./Component";

export abstract class CoreComponent extends Component
{
	public static readonly getIndex = <T extends Component>(type: ComponentType<T>) => (type as any)._internalIndex;

	public readonly getIndex = () => (this.constructor as any)._internalIndex;
}