namespace N97 {
  export const base: number = 97;
  export function add(x: number): number { return base + x; }
}
function over97(x: number): number;
function over97(x: string): string;
function over97(x: number | string): number | string { return x; }
abstract class A97 { abstract f(x: number): number; }
class B97 extends A97 { override f(x: number): number { return x + 1; } }
enum E97 { A = 5 / 2, B, C = 2 ** 3, D }
const angle97 = <number>(97 + 1);
const check97 = {x: angle97} satisfies {x: number};
console.log(N97.add(1), over97(97), new B97().f(1), E97.B, E97.D, check97.x);
