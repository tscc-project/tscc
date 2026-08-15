interface Item321 { id: number; name: string; }
type Key321 = string | number;
export function make321(id: number, name: string): Item321 {
  const value: Item321 = {id, name};
  return value as Item321;
}
export const item321: Item321 = make321(321, "item-321");
