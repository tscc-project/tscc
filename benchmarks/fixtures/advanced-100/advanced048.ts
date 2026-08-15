namespace N48 {
  export const base: number = 48;
  export function add(x: number): number { return base + x; }
}
function over48(x: number): number;
function over48(x: string): string;
function over48(x: number | string): number | string { return x; }
abstract class A48 { abstract f(x: number): number; }
class B48 extends A48 { override f(x: number): number { return x + 1; } }
enum E48 { A = 5 / 2, B, C = 2 ** 3, D }
const angle48 = <number>(48 + 1);
const check48 = {x: angle48} satisfies {x: number};
console.log(N48.add(1), over48(48), new B48().f(1), E48.B, E48.D, check48.x);
