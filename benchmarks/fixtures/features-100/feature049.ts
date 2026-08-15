function id49<T>(x: T): T { return x; }
class Base49<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child49 extends Base49<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E49 { A, B = 3, C, Label = "label" }
const x49: number = id49<number>(49);
const c49 = new Child49("n", x49);
console.log(c49.name, E49.A, E49[3]);
