interface Item399 { id: number; name: string; }
type Key399 = string | number;
export function make399(id: number, name: string): Item399 {
  const value: Item399 = {id, name};
  return value as Item399;
}
export const item399: Item399 = make399(399, "item-399");
