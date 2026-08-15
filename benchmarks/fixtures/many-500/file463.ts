interface Item463 { id: number; name: string; }
type Key463 = string | number;
export function make463(id: number, name: string): Item463 {
  const value: Item463 = {id, name};
  return value as Item463;
}
export const item463: Item463 = make463(463, "item-463");
