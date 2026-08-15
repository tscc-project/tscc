interface Item442 { id: number; name: string; }
type Key442 = string | number;
export function make442(id: number, name: string): Item442 {
  const value: Item442 = {id, name};
  return value as Item442;
}
export const item442: Item442 = make442(442, "item-442");
