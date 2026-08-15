namespace N27 {
  export const base: number = 27;
  export function add(x: number): number { return base + x; }
}
function over27(x: number): number;
function over27(x: string): string;
function over27(x: number | string): number | string { return x; }
abstract class A27 { abstract f(x: number): number; }
class B27 extends A27 { override f(x: number): number { return x + 1; } }
enum E27 { A = 5 / 2, B, C = 2 ** 3, D }
const angle27 = <number>(27 + 1);
const check27 = {x: angle27} satisfies {x: number};
console.log(N27.add(1), over27(27), new B27().f(1), E27.B, E27.D, check27.x);
