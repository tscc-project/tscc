function id27<T>(x: T): T { return x; }
class Base27<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child27 extends Base27<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E27 { A, B = 3, C, Label = "label" }
const x27: number = id27<number>(27);
const c27 = new Child27("n", x27);
console.log(c27.name, E27.A, E27[3]);
