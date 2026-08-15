interface Item404 { id: number; name: string; }
type Key404 = string | number;
export function make404(id: number, name: string): Item404 {
  const value: Item404 = {id, name};
  return value as Item404;
}
export const item404: Item404 = make404(404, "item-404");
