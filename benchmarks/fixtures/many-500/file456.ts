interface Item456 { id: number; name: string; }
type Key456 = string | number;
export function make456(id: number, name: string): Item456 {
  const value: Item456 = {id, name};
  return value as Item456;
}
export const item456: Item456 = make456(456, "item-456");
