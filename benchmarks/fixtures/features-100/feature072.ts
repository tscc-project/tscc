function id72<T>(x: T): T { return x; }
class Base72<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child72 extends Base72<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E72 { A, B = 3, C, Label = "label" }
const x72: number = id72<number>(72);
const c72 = new Child72("n", x72);
console.log(c72.name, E72.A, E72[3]);
