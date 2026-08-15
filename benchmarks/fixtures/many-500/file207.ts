interface Item207 { id: number; name: string; }
type Key207 = string | number;
export function make207(id: number, name: string): Item207 {
  const value: Item207 = {id, name};
  return value as Item207;
}
export const item207: Item207 = make207(207, "item-207");
