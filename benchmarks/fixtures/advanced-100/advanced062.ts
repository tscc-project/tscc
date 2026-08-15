namespace N62 {
  export const base: number = 62;
  export function add(x: number): number { return base + x; }
}
function over62(x: number): number;
function over62(x: string): string;
function over62(x: number | string): number | string { return x; }
abstract class A62 { abstract f(x: number): number; }
class B62 extends A62 { override f(x: number): number { return x + 1; } }
enum E62 { A = 5 / 2, B, C = 2 ** 3, D }
const angle62 = <number>(62 + 1);
const check62 = {x: angle62} satisfies {x: number};
console.log(N62.add(1), over62(62), new B62().f(1), E62.B, E62.D, check62.x);
