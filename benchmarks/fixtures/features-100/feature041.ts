function id41<T>(x: T): T { return x; }
class Base41<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child41 extends Base41<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E41 { A, B = 3, C, Label = "label" }
const x41: number = id41<number>(41);
const c41 = new Child41("n", x41);
console.log(c41.name, E41.A, E41[3]);
