function id73<T>(x: T): T { return x; }
class Base73<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child73 extends Base73<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E73 { A, B = 3, C, Label = "label" }
const x73: number = id73<number>(73);
const c73 = new Child73("n", x73);
console.log(c73.name, E73.A, E73[3]);
