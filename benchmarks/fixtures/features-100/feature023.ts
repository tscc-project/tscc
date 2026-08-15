function id23<T>(x: T): T { return x; }
class Base23<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child23 extends Base23<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E23 { A, B = 3, C, Label = "label" }
const x23: number = id23<number>(23);
const c23 = new Child23("n", x23);
console.log(c23.name, E23.A, E23[3]);
