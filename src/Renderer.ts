import { Engine } from "./Engine";

export class Renderer
{
	private static readonly GPU = window.navigator.gpu;

	private _isInitialized: boolean = false;

	public readonly engine: Engine;
	
	private _adapter: GPUAdapter | null = null;

	public get adapter(): GPUAdapter
	{
		if(!this._adapter)
			throw new Error(`Could not get GPUAdapter!`);
		return this._adapter;
	}

	private _device: GPUDevice | null = null;

	public get device(): GPUDevice
	{
		if(!this._device)
			throw new Error(`Could not get GPUDevice!`);
		return this._device;
	}

	public get isInitialized() { return this._isInitialized; }

	public constructor(engine: Engine)
	{
		this.engine = engine;
	}

	public readonly initialize = async () =>
	{
		if (this.isInitialized)
			throw new Error("renderer is already initialized!");

		const gpu = Renderer.GPU;

		if (!gpu)
			throw new Error("Could not get WebGPU context!");

		const adapter = await gpu.requestAdapter({ powerPreference: "high-performance" });
		if (!adapter)
			throw new Error("Could not get an WebGPU adapter!");

		this._adapter = adapter;
	}

	private readonly initDevice = async () =>
	{
		const device = await this.adapter.requestDevice({

		});

		if (!device)
			throw new Error("Could not get WebGPU device!");

		this._device = device;

		device.lost.then(async () => 
		{
			await this.initDevice();
		});
	}
}