interface Item90 { id: number; name: string; }
type Key90 = string | number;
export function make90(id: number, name: string): Item90 {
  const value: Item90 = {id, name};
  return value as Item90;
}
export const item90: Item90 = make90(90, "item-90");
