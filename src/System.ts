export interface ISystem<Events extends { [key: string]: any }>
{
	on<K extends keyof Events>(event: K, callback: (data: Events[K]) => void): void;
	testEvent<K extends keyof Events>(event: K, data: Events[K]): void;
}

export interface IRenderer extends ISystem<{ test: { test: boolean } }> {} 