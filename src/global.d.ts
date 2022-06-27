declare const env: {
	isDev: boolean;
};

declare type RequiredKeys<T> = { [K in keyof T]-?: {} extends Pick<T, K> ? never : K }[keyof T];
declare type OptionalKeys<T> = { [K in keyof T]-?: {} extends Pick<T, K> ? K : never }[keyof T];

declare type ObjectMap<T = any> = { [key: string]: T };
declare type Transform<T, Type> = { [P in keyof T]: Type };
declare type Nullable<T> = T | null;
declare type NullableMap<T> = { [P in keyof T]: T[P] | null };
declare type OptionalMap<T> = { [P in keyof T]?: T[P] };
declare interface ReactBaseProps
{
	id?: string;
	className?: string;
	style?: React.CSSProperties;
}

declare type ReactBasePropsWithChildren = React.PropsWithChildren<ReactBaseProps>;

type RecursiveReadonly<T> = T extends boolean ? T : T extends number ? T : T extends Array<infer R> ? ReadonlyArray<R> : T extends string ? T : T extends Symbol ? T : T extends Function ? T : T extends {} ? {
	readonly [K in keyof T]: RecursiveReadonly<T[K]>;
} : Readonly<T>;