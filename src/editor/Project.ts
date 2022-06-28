import { existsSync } from "fs";

export class Project
{
	public name: string;
	public readonly path: string;

	public constructor(props: ProjectProps)
	{
		this.name = props.name;
		if(!existsSync(props.path))
			throw new Error(`${props.path} is not a game!`);
		this.path = props.path;
	}
}

export type ProjectProps = {
	name: string;
	path: string;
};
