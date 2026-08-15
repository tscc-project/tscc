function id99<T>(x: T): T { return x; }
class Base99<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child99 extends Base99<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E99 { A, B = 3, C, Label = "label" }
const x99: number = id99<number>(99);
const c99 = new Child99("n", x99);
console.log(c99.name, E99.A, E99[3]);
