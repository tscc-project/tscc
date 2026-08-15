function id19<T>(x: T): T { return x; }
class Base19<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child19 extends Base19<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E19 { A, B = 3, C, Label = "label" }
const x19: number = id19<number>(19);
const c19 = new Child19("n", x19);
console.log(c19.name, E19.A, E19[3]);
