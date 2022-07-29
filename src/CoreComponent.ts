import { Component, ComponentType } from "./Component";

export abstract class CoreComponent extends Component
{
	public static readonly getIndex = <T extends Component>(type: ComponentType<T>) => (type as any).internalIndex_;

	public readonly getIndex = () => (this.constructor as any).internalIndex_;
}