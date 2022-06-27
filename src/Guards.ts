export namespace Guards
{
	export const editor = (target: any, propertyKey: string, descriptor?: PropertyDescriptor) =>
	{
		if (!env.isEditor)
			return () => { throw new Error(`Cannot call editor method from game!`); };
		return target[propertyKey];
	};

	export const game = (target: any, propertyKey: string, descriptor?: PropertyDescriptor) =>
	{
		if (env.isEditor)
			return target[propertyKey] = () => { throw new Error(`Cannot call game method from editor!`); };
		return target[propertyKey];
	}
}