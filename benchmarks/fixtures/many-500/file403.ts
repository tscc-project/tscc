interface Item403 { id: number; name: string; }
type Key403 = string | number;
export function make403(id: number, name: string): Item403 {
  const value: Item403 = {id, name};
  return value as Item403;
}
export const item403: Item403 = make403(403, "item-403");
