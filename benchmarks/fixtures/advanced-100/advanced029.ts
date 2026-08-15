namespace N29 {
  export const base: number = 29;
  export function add(x: number): number { return base + x; }
}
function over29(x: number): number;
function over29(x: string): string;
function over29(x: number | string): number | string { return x; }
abstract class A29 { abstract f(x: number): number; }
class B29 extends A29 { override f(x: number): number { return x + 1; } }
enum E29 { A = 5 / 2, B, C = 2 ** 3, D }
const angle29 = <number>(29 + 1);
const check29 = {x: angle29} satisfies {x: number};
console.log(N29.add(1), over29(29), new B29().f(1), E29.B, E29.D, check29.x);
