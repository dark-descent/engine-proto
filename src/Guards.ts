export namespace Guards
{
	export const editor = (target: any, propertyKey: string, descriptor?: PropertyDescriptor) =>
	{
		if (!env.isEditor)
			return {
				get() { throw new Error(`Cannot call ${target.name}.${propertyKey} method from game!`); }
			};
		return target[propertyKey];
	};

	export const game = (target: any, propertyKey: string, descriptor?: PropertyDescriptor) =>
	{
		if (env.isEditor)
			return {
				get() { throw new Error(`Cannot call ${target.name}.${propertyKey} from editor!`); }
			};
		return target[propertyKey];
	}
}