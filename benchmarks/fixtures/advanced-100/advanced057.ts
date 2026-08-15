namespace N57 {
  export const base: number = 57;
  export function add(x: number): number { return base + x; }
}
function over57(x: number): number;
function over57(x: string): string;
function over57(x: number | string): number | string { return x; }
abstract class A57 { abstract f(x: number): number; }
class B57 extends A57 { override f(x: number): number { return x + 1; } }
enum E57 { A = 5 / 2, B, C = 2 ** 3, D }
const angle57 = <number>(57 + 1);
const check57 = {x: angle57} satisfies {x: number};
console.log(N57.add(1), over57(57), new B57().f(1), E57.B, E57.D, check57.x);
