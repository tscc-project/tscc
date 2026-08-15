interface Item443 { id: number; name: string; }
type Key443 = string | number;
export function make443(id: number, name: string): Item443 {
  const value: Item443 = {id, name};
  return value as Item443;
}
export const item443: Item443 = make443(443, "item-443");
