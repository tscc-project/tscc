function id86<T>(x: T): T { return x; }
class Base86<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child86 extends Base86<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E86 { A, B = 3, C, Label = "label" }
const x86: number = id86<number>(86);
const c86 = new Child86("n", x86);
console.log(c86.name, E86.A, E86[3]);
