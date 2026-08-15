interface Item150 { id: number; name: string; }
type Key150 = string | number;
export function make150(id: number, name: string): Item150 {
  const value: Item150 = {id, name};
  return value as Item150;
}
export const item150: Item150 = make150(150, "item-150");
