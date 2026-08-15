interface Item118 { id: number; name: string; }
type Key118 = string | number;
export function make118(id: number, name: string): Item118 {
  const value: Item118 = {id, name};
  return value as Item118;
}
export const item118: Item118 = make118(118, "item-118");
