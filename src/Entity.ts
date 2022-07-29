import { ComponentType } from "react"
import { Component } from "./Component"

export class Entity
{
	public constructor()
	{

	}

	public readonly addComponent = <T extends Component>(type: ComponentType<T>) =>
	{

	}

	public readonly getComponent = <T extends Component>(type: ComponentType<T>) =>
	{

	}

	public readonly removeComponent = <T extends Component>(type: ComponentType<T>) =>
	{
		
	}
}