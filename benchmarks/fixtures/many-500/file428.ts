interface Item428 { id: number; name: string; }
type Key428 = string | number;
export function make428(id: number, name: string): Item428 {
  const value: Item428 = {id, name};
  return value as Item428;
}
export const item428: Item428 = make428(428, "item-428");
