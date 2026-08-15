namespace N58 {
  export const base: number = 58;
  export function add(x: number): number { return base + x; }
}
function over58(x: number): number;
function over58(x: string): string;
function over58(x: number | string): number | string { return x; }
abstract class A58 { abstract f(x: number): number; }
class B58 extends A58 { override f(x: number): number { return x + 1; } }
enum E58 { A = 5 / 2, B, C = 2 ** 3, D }
const angle58 = <number>(58 + 1);
const check58 = {x: angle58} satisfies {x: number};
console.log(N58.add(1), over58(58), new B58().f(1), E58.B, E58.D, check58.x);
