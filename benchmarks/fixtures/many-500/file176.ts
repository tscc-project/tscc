interface Item176 { id: number; name: string; }
type Key176 = string | number;
export function make176(id: number, name: string): Item176 {
  const value: Item176 = {id, name};
  return value as Item176;
}
export const item176: Item176 = make176(176, "item-176");
