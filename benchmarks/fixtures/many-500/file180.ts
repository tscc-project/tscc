interface Item180 { id: number; name: string; }
type Key180 = string | number;
export function make180(id: number, name: string): Item180 {
  const value: Item180 = {id, name};
  return value as Item180;
}
export const item180: Item180 = make180(180, "item-180");
