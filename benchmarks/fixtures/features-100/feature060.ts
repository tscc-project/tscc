function id60<T>(x: T): T { return x; }
class Base60<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child60 extends Base60<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E60 { A, B = 3, C, Label = "label" }
const x60: number = id60<number>(60);
const c60 = new Child60("n", x60);
console.log(c60.name, E60.A, E60[3]);
