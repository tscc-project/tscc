interface Item130 { id: number; name: string; }
type Key130 = string | number;
export function make130(id: number, name: string): Item130 {
  const value: Item130 = {id, name};
  return value as Item130;
}
export const item130: Item130 = make130(130, "item-130");
