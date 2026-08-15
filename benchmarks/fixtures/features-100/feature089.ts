function id89<T>(x: T): T { return x; }
class Base89<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child89 extends Base89<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E89 { A, B = 3, C, Label = "label" }
const x89: number = id89<number>(89);
const c89 = new Child89("n", x89);
console.log(c89.name, E89.A, E89[3]);
